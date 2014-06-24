<?php
/**
 * @file
 * Migrate class to import the BookPage nodes.
 */

/**
 * Class to import the Book (Page) nodes from the Drupal 6 platform.
 */
class C4dMigrateImportNodeBookPage extends C4dMigrateImportBaseNode {
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

    $this->description = t('Import topics from our Drupal 6 database.');

    $query = $this->createSelect('c4d_export_node_page', 't')
                  ->orderBy('nid', 'ASC')
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
         ->arguments(array('format' => 'full_html'));
  }

  /**
   * Review a data row after fetch, returning FALSE to skip it.
   *
   * @param object $node
   *   The node object we are creating.
   * @param object $row
   *   The data row we are importing.
   */
  public function prepare($node, $row) {
    $node->book = array(
      'pick-book'          => 'Change book (update list of parents)',
      'bid'                => 'new',
      'has_children'       => 0,
      'options'            => array(),
      'module'             => 'book',
      'original_bid'       => 0,
      'parent_depth_limit' => 8,
      'weight'             => '0',
    );
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
  }
}
