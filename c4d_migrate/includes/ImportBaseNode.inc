<?php
/**
 * @file
 * Base class to import data into nodes.
 */

/**
 * Base class to import data into nodes.
 *
 * This base class has the common functionality to import nodes:
 *   - It creates the query on the source table based on the sourceTable.
 *   - It creates the migration source object.
 *   - It creates the migration target object based on the targetBundle.
 *   - It creates the migration mapping object.
 *   - It defines the default field mapping for nodes.
 *
 * A subclass has only 3 methods to implement:
 *   - __construct() : to set variables like sourceTable, targetBundle and if
 *       the path should be created auto or from the migration data.
 *   - addCustomFieldMapping() : map the fields that are not common to nodes.
 *       WARNING: you need to define the body field also here; not every node
 *                type has a body field.
 *   - prepareRow() : this is optional; "preprocess" the row data.
 *
 * The subclass needs to define 2 variables in the class constructor before
 * calling the parent:
 *   - $this->setSourceTable() : Set the source table with the data that should
 *       be imported.
 *   - $this->setTargetBundle() : Set the target node bundle where we need to
 *       import the data.
 *
 * There is a third optional parameter that can be set:
 * should the node path be created automatically or should the value from the
 * migration table be used:
 *   - $this->setPathManual() : Default FALSE.
 *
 * Example:
 * <code>
 * public function __construct($group = NULL) {
 *   $this->description = t('Import Region nodes.');
 *
 *   $this->setSourceTable('c4d_export_node_geo_region');
 *   $this->setTargetBundle('region');
 *
 *   // Optionally.
 *   $this->setPathManual(TRUE);
 *
 *   // Important: call the parent after setting the source & target!
 *   parent::__construct($group);
 * }
 * </code>
 */
abstract class C4dMigrateImportBaseNode extends C4dMigrateImportBase {
  /**
   * The source database table we want to import from.
   *
   * @var string
   */
  private $sourceTable;

  /**
   * The node bundle we want to import to.
   *
   * @var string
   */
  private $targetBundle;

  /**
   * Should the node path be set manually (from the node export data).
   *
   * @var bool
   */
  private $pathManual = FALSE;

  /**
   * The legacy Flag ID for nodes.
   *
   * @var bool
   */
  private $legacyFlagId = FALSE;

  /**
   * The Flag ID for nodes.
   *
   * @var bool
   */
  private $flagId = FALSE;

  /**
   * General initialization of a Migration object.
   *
   * This implements the common functionality to import nodes from a flat table.
   *
   * @param object $group
   *   The migrate Group object.
   * @param bool $skip_func
   *   Skip function.
   */
  public function __construct($group = NULL, $skip_func = FALSE) {
    parent::__construct($group);

    if ($skip_func) {
      return;
    }

    // Define the source table to import the data from.
    $query = $this->createSelect($this->getSourceTable(), 'node')
      ->fields('node');
    $this->source = new MigrateSourceSQL(
      $query,
      array(),
      NULL,
      array('map_joinable' => FALSE)
    );

    // Set the destination node bundle to import the data to.
    $destination_handler = new MigrateDestinationNode($this->getTargetBundle());

    // Add map to track relationships between source & destination rows.
    $key = array(
      'nid' => array(
        'type' => 'int',
        'not null' => TRUE,
        'unsigned' => TRUE,
      ),
    );
    $this->map = new MigrateSQLMap(
      $this->machineName,
      $key,
      $destination_handler->getKeySchema()
    );
    $this->destination = $destination_handler;

    $this->setPathManual(TRUE);

    // Map the fields.
    $this->addDefaultFieldMapping();
    $this->addCustomFieldMapping();

    // Make sure no revisions are created during import.
    $this->disableRevisions();
  }

  /**
   * Complete function to set missing aliases.
   *
   * @param object $node
   *   Drupal node object.
   * @param object $row
   *   Migration source row object.
   */
  public function complete($node, $row) {
    $query = $this->createSelect('url_alias', 'u')
      ->orderBy('src', 'ASC')
      ->fields('u', array('pid', 'src', 'dst'))
      ->condition('src', 'node/' . $row->nid)
      ->groupBy('dst')
      ->groupBy('src')
      ->groupBy('pid')
      ->execute();
    $results = $query->fetchAll();

    foreach ($results as $result) {
      if ($result->dst != $row->path && !$this->getPathManual()) {
        $redirect = new stdClass();

        redirect_object_prepare(
          $redirect,
          array(
            'uid' => $node->uid,
            'source' => $result->dst,
            'source_options' => array(),
            'redirect' => 'node/' . $node->nid,
            'redirect_options' => array(),
            'language' => LANGUAGE_NONE,
          )
        );

        $rid = redirect_load_by_hash(redirect_hash($redirect));
        if (empty($rid)) {
          redirect_save($redirect);
        }
      }
    }

    // Find the legacy flag ID.
    $legacy_flag_id = $this->getLegacyFlagId();
    // Find the flag ID.
    $flag_id = $this->getFlagId();

    // Find if the current legacy node has flags.
    if ($legacy_flag_id && $flag_id) {
      // Flagging.
      $query = $this->createSelect('flag_content', 'fc')
        ->orderBy('fcid', 'ASC')
        ->fields('fc', array('content_id', 'uid', 'sid', 'timestamp'))
        ->condition('fid', $legacy_flag_id)
        ->condition('content_type', 'node')
        ->condition('content_id', $row->nid)
        ->execute();
      $results = $query->fetchAll();

      foreach ($results as $result) {
        $uid = $this->handleSourceMigration(
          'C4dMigrateImportUsers',
          $result->uid,
          NULL,
          $this
        );
        $record = array(
          'fid'         => $flag_id,
          'entity_type' => 'node',
          'entity_id'   => $node->nid,
          'uid'         => $uid,
          'sid'         => $result->sid,
          'timestamp'   => $result->timestamp,
        );
        drupal_write_record('flagging', $record);
      }

      // Flag counts.
      $query = $this->createSelect('flag_counts', 'fc')
        ->fields('fc', array('count', 'last_updated'))
        ->condition('fid', $legacy_flag_id)
        ->condition('content_type', 'node')
        ->condition('content_id', $row->nid)
        ->execute();
      $results = $query->fetchAll();

      foreach ($results as $result) {
        $record = array(
          'fid' => $flag_id,
          'entity_type' => 'node',
          'entity_id' => $node->nid,
          'count' => $result->count,
          'last_updated' => $result->last_updated,
        );
        drupal_write_record('flag_counts', $record);
      }
    }
  }

  /**
   * Helper to disable revisions during node import.
   */
  public function disableRevisions() {
    $variable = variable_get(
      'node_options_' . $this->getTargetBundle(),
      array()
    );

    $key = array_search('revision', $variable);
    if ($key !== FALSE) {
      unset($variable[$key]);
      variable_set('node_options_' . $this->getTargetBundle(), $variable);

      drupal_flush_all_caches();
    }
  }

  /**
   * Helper to enable the revisions after the node import.
   */
  public function enableRevisions() {
    $variable = variable_get(
      'node_options_' . $this->getTargetBundle(),
      array()
    );

    $key = array_search('revision', $variable);
    // Has revisions enabled?
    if ($key === FALSE) {
      return;
    }

    unset($variable[$key]);
    variable_set('node_options_' . $this->getTargetBundle(), $variable);

    drupal_flush_all_caches();
  }

  /**
   * Add field mapping for the non-common node fields.
   */
  public function addCustomFieldMapping() {
  }

  /**
   * Add the mapping for the common node fields.
   */
  public function addDefaultFieldMapping() {
    $this->addFieldMapping('title', 'title');
    $this->addFieldMapping('status', 'status');
    $this->addFieldMapping('created', 'created');
    $this->addFieldMapping('changed', 'changed');
    $this->addFieldMapping('comment', 'comment');
    $this->addFieldMapping('promote', 'promote');

    // Node author.
    $this->addFieldMapping('uid', 'uid')
      ->sourceMigration('C4dMigrateImportUsers');

    // Should the node path be set manually?
    if ($this->getPathManual()) {
      $this->addFieldMapping('pathauto')->defaultValue(0);
      $this->addFieldMapping('path', 'path');
    }
  }

  /**
   * Set the source table name.
   *
   * @param string $table
   *   The source table name.
   *
   * @return C4dMigrateImportBaseNode
   *   Support for fluent interface.
   */
  public function setSourceTable($table) {
    $this->sourceTable = $table;

    return $this;
  }

  /**
   * Get the source table name.
   *
   * @return string
   *   The name of the source table.
   */
  public function getSourceTable() {
    return $this->sourceTable;
  }

  /**
   * Set the bundle we want to import to.
   *
   * @param string $bundle
   *   The target node bundle name.
   *
   * @return C4dMigrateImportBaseNode
   *   Support for fluent interface.
   */
  public function setTargetBundle($bundle) {
    $this->targetBundle = $bundle;

    return $this;
  }

  /**
   * Get the bundle we want to import to.
   *
   * @return string
   *   The name of the target node bundle.
   */
  public function getTargetBundle() {
    return $this->targetBundle;
  }

  /**
   * Change the way that node path will be set, manually or automatically.
   *
   * The default is that the pathauto module is used to create the node paths.
   * The exported migration data contains the path on the old platform. That
   * can be reused by setting the setPathManual = TRUE.
   *
   * @param bool $state
   *   Should it be manually (TRUE) or not (FALSE).
   *
   * @return C4dMigrateImportBaseNode
   *   Support for fluent interface.
   */
  public function setPathManual($state) {
    $this->pathManual = (bool) $state;

    return $this;
  }

  /**
   * Check if the node path should be set manually.
   *
   * @return bool
   *   TRUE if the path needs to be set manually.
   */
  public function getPathManual() {
    return $this->pathManual;
  }

  /**
   * Returns the legacy Flag ID for nodes.
   *
   * @return bool | integer
   */
  public function getLegacyFlagId() {
    if (!$this->legacyFlagId) {
      $query = $this->createSelect('flags', 'f')
        ->fields('f', array('fid'))
        ->condition('content_type', 'node')
        ->condition('name', 'recommend_content')
        ->execute();
      $results = $query->fetchAll();

      if (count($results) && isset($results[0]) && isset($results[0]->fid)) {
        $this->legacyFlagId = $results[0]->fid;
      }
    }

    return $this->legacyFlagId;
  }

  /**
   * Returns the legacy Flag ID for nodes.
   *
   * @return bool | integer
   */
  public function getFlagId() {
    if (!$this->flagId) {
      $flag = flag_get_flag('c4m_og_content_recommend');

      if ($flag) {
        $this->flagId = $flag->fid;
      }
    }

    return $this->flagId;
  }

  /**
   * Reset the $node->changed timestamp.
   *
   * @param object $node
   *   Node object we want to reset the changed date for.
   * @param int $changed
   *   Optional timestamp.
   */
  public function nodeSave($node, $changed = NULL) {
    $node->revision = FALSE;

    if (empty($node->migrate)) {
      // Flag to set revisions to false in hook_presave.
      $node->migrate = TRUE;
    }

    node_save($node);

    if ($changed) {
      db_update('node')->fields(
        array('changed' => $changed, 'uid' => $node->uid)
      )
        ->condition('nid', $node->nid, '=')
        ->execute();
      // Drush acts as user 0.
      db_update('node_revision')->fields(
        array('timestamp' => $changed, 'uid' => $node->uid)
      )
        ->condition('nid', $node->nid, '=')
        ->condition('vid', $node->vid, '=')
        ->execute();
    }
  }

  /**
   * Reset the $message->timestamp of any related insert message (if any).
   *
   * @param object $node
   *   Node object we want to set the message timestamp for.
   * @param object $row
   *   The source data of the migrated entity.
   */
  public function messageInsertSave($node, $row) {
    if (!module_exists('c4m_message')) {
      return;
    }

    // Load the message(s).
    $message_type = 'c4m_insert__node__' . $node->type;
    $query = new EntityFieldQuery();
    $result = $query
      ->entityCondition('entity_type', 'message')
      ->propertyCondition('type', $message_type)
      ->fieldCondition('field_node', 'target_id', $node->nid)
      ->execute();

    if (empty($result['message'])) {
      return;
    }

    // Update the timestamp.
    $message_ids = array_keys($result['message']);
    foreach ($message_ids as $message_id) {
      $message = message_load($message_id);
      if (!$message) {
        continue;
      }

      $message->timestamp = $row->created;
      message_save($message);
    }
  }

  /**
   * Create optional update messages.
   *
   * @param object $node
   *   Node object we want to set the message timestamp for.
   * @param object $row
   *   The source data of the migrated entity.
   */
  public function messageUpdateSave($node, $row) {
    if (!module_exists('c4m_message')) {
      return;
    }

    if ($row->changed <= $row->created) {
      return;
    }

    // Retrieve the mapped changed_user.
    $old_user_id = $row->changed_uid;

    if (empty($old_user_id)) {
      return;
    }
    $new_user_id = $this->handleSourceMigration(
      'C4dMigrateImportUsers',
      $old_user_id,
      NULL,
      $this
    );

    $account = user_load($new_user_id);

    $message = c4m_message__update__node($node, $account);

    $message->timestamp = $row->changed;
    $message->user = $account;
    $message->uid = $account->uid;
    message_save($message);
  }

}
