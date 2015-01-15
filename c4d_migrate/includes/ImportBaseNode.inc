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
   * General initialization of a Migration object.
   *
   * This implements the common functionality to import nodes from a flat table.
   *
   * @param MigrateGroup $group
   *   The migrate Group object.
   */
  public function __construct($group = NULL) {
    parent::__construct($group);

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

    // make sure no revisions are created during import.
    $this->disableRevisions();
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
      unset ($variable[$key]);
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

    unset ($variable[$key]);
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
   * Reset the $node->changed timestamp.
   *
   * @param object $node
   *   Node object we want to reset the changed date for.
   * @param int    $changed
   *   Timestamp.
   */
  public function nodeSave($node, $changed = NULL) {
    $node->revision = FALSE;

    if (empty($node->migrate)) {
      $node->migrate = TRUE; // Flag to set revisions to false in hook_presave.
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
