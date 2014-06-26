<?php
/**
 * @file
 * Migrate class to import the Topic nodes.
 */

/**
 * Class to import the Topic nodes from the Drupal 6 platform.
 */
class C4dMigrateImportNodeTopic extends C4dMigrateImportBaseNode {
  public $entityType = 'node';
  public $bundle = 'topic';

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

    $query = $this->createSelect('c4d_export_node_topic', 't')
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

    // Map the picture field.
    $this->addFileFieldMapping('c4m_media', 'field_picture');
    // Other fields are not described yet for the Drupal 7 Content Type.
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

    // Prepare the picture data.
    $this->prepareFileInRow($row, 'field_picture');

    $row->body = $this->prepareHtml($row->body);

    return TRUE;
  }

  /**
   * Create stub Topics (when creating users), who will be replaced afterwards.
   *
   * @param Migration $migration
   *   The migration that is being done to create this stub.
   * @param array     $source_id
   *   Array of strings with the source id(s).
   *
   * @return array|bool
   *   Array with topic node id if term created, else FALSE.
   */
  public function createStub(Migration $migration, array $source_id) {
    $node                         = new stdClass();
    $node->title                  = t('Stub for @id', array('@id' => $source_id[0]));
    $node->body[LANGUAGE_NONE][0] = t('Stub topic');
    $node->type                   = $this->destination->getBundle();
    $node->uid                    = 1;
    $node->status                 = 0;
    $node->revision               = FALSE;
    node_save($node);
    if (isset($node->nid)) {
      return array($node->nid);
    }
    else {
      return FALSE;
    }
  }
}
