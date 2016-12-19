<?php
/**
 * @file
 * Migrate class to import the HelpPage nodes.
 */

/**
 * Class to import the Help (Page) nodes from the Drupal 6 platform.
 */
class C4dMigrateImportNodeHelpPage extends C4dMigrateImportBaseNode {
  public $entityType = 'node';
  public $bundle = 'book';

  /**
   * Content creations that should run before this one.
   *
   * @var array
   */
  public $dependencies = array(
    'C4dMigrateImportUsers',
  );

  /**
   * Configure the object during creation.
   */
  public function __construct() {
    parent::__construct(
          MigrateGroup::getInstance(C4D_MIGRATE_MIGRATION_GROUP)
    );

    $this->description = t('Import help pages from our Drupal 6 database.');

    // Order by parent because nodes could have been rearranged, causing
    // Children to be created before their parents otherwise, creating execution
    // errors.
    $query = $this->createSelect('c4d_export_node_help', 't')
                  ->orderBy('weight', 'ASC')
                  ->fields('t');

    $this->source = new MigrateSourceSQL($query, array(), NULL, array('map_joinable' => FALSE));

    // Create a map object for tracking the relationships between source rows.
    $key = array(
      'nid' => array(
        'type'     => 'varchar',
        'length'   => 255,
        'not null' => TRUE,
      ),
    );

    // Automatically convert drupal passwords to drupal7 hash.
    $destination_handler = new MigrateDestinationNode($this->bundle);

    $this->map = new MigrateSQLMap(
      $this->machineName,
      $key,
      $destination_handler->getKeySchema()
    );

    $this->destination = $destination_handler;
  }

  /**
   * {@inheritdoc}
   */
  public function addCustomFieldMapping() {
    // Map the custom fields.
    $this->addFieldMapping('c4m_body', 'body')
         ->arguments(array('format' => 'filtered_html'));
  }

  /**
   * Review a data row after fetch, returning FALSE to skip it.
   *
   * @param object $node
   *   The node object we are creating.
   * @param object $row
   *   The data row we are importing.
   */
  public function complete($node, $row) {
    parent::complete($node, $row);

    // Books are not loaded by default.
    $node = node_load($node->nid);

    // Take care of import --update.
    // Now there's an Integrity constraint violation
    // Duplicate entry for key nid.
    $update = FALSE;
    if (!empty($node->book) && !empty($node->book['nid'])) {
      if ($node->book['nid'] !== 'new' || intval($node->book['nid']) != 0) {
        $update = TRUE;
      }
    }

    // If no parent - we need to create a new book.
    $parent = $row->parent;
    if (empty($parent)) {
      // No parent: this is the main "Help & Guidance" page, so create book.
      if (!$update) {
        // This is a new migration, no update, so create a new book.
        $node->book = array(
          'pick-book'          => 'Change book (update list of parents)',
          'bid'                => 'new',
          'has_children'       => 0,
          'options'            => array(),
          'module'             => 'book',
          'original_bid'       => 0,
          'parent_depth_limit' => 8,
          'weight'             => $row->weight,
          'nid'                => $node->nid,
        );
        $this->nodeSave($node, $row->changed);
      }
      else {
        // This is an update, so we retrieve the existing book.
        $book = $node->book;
        // Only value for the master book page that can change is the weight.
        if ($book['weight'] != $row->weight) {
          $book['weight'] = $row->weight;
          $node->book     = $book;
          $this->nodeSave($node, $row->changed);
        }
      }
    }
    else {
      // Create a nested page.
      $parent_nid = $this->getParentNid($parent);
      $parent     = node_load($parent_nid);

      $book = $parent->book;

      $book['plid'] = $book['mlid'];
      $book['nid']  = $node->nid;

      if ($update) {
        // There is already a {menu_links} entry for this node.
        $book['mlid'] = $this->getMlidEntry($node->nid);
      }
      else {
        unset($book['mlid']);
      }
      $book['weight'] = $row->weight;

      $node->book = $book;

      _book_update_outline($node);
      $this->nodeSave($node, $row->changed);
    }
  }

  /**
   * Get the migrated parent id.
   *
   * @param int $nid
   *   Source id of the parent.
   *
   * @return int|null
   *   Parent id or NULL.
   */
  private function getParentNid($nid) {
    $parent = $this->handleSourceMigration(
                   'C4dMigrateImportNodeHelpPage',
                     $nid,
                     NULL,
                     $this
    );
    if (!empty($parent)) {
      return $parent;
    }

    return NULL;
  }

  /**
   * Based on the node id, retrieve the existing {menu_links} mlid.
   *
   * @param int $nid
   *   Node id to retrieve the mlid for.
   *
   * @return int|NULL
   *   ID of the {menu_links} entry or NULL.
   */
  private function getMlidEntry($nid) {
    $q   = db_select('book', 'book')
           ->fields('book', array('mlid'))
           ->condition('book.nid', $nid, '=')
           ->range(0, 1);
    $res = $q->execute()->fetchObject();

    if (!empty($res->mlid)) {
      return $res->mlid;
    }

    return NULL;
  }

  /**
   * Prepare the row data loaded from the table.
   *
   * @param object $row
   *   The data from the table.
   *
   * @return bool
   *   Success.
   */
  public function prepareRow($row) {
    parent::prepareRow($row);

    $row->body = $this->prepareHtml($row->body);
    return TRUE;
  }

}
