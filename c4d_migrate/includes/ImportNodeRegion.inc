<?php
/**
 * @file
 * Migrate class to import the Region nodes.
 */

/**
 * Class to import the Region nodes from the Drupal 6 platform.
 */
class C4dMigrateImportNodeRegion extends C4dMigrateImportBase {
  /**
   * Creation classes that should run before this one.
   *
   * @var array
   */
  public $dependencies = array(
    'C4dMigrateImportVocabGeo',
    //'C4dMigrateImportUsers',
  );


  /**
   * Configure the object during creation.
   */
  public function __construct() {
    parent::__construct(
      MigrateGroup::getInstance(C4D_MIGRATE_MIGRATION_GROUP)
    );

    $this->description = t('Import Region nodes.');

    // Define the source.
    $query = $this->createSelect('c4d_export_node_geo_region', 'region')
                  ->fields('region');
    $this->source = new MigrateSourceSQL(
      $query,
      array(),
      NULL,
      array('map_joinable' => FALSE)
    );

    // Import into Region node bundle.
    $destination_handler = new MigrateDestinationNode('region');

    // Create a map object for tracking the
    // relationships between source & destination rows.
    $key = array(
      'nid' => array(
        'type'     => 'int',
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

    // Map the fields.
    $this->addFieldMapping('title', 'title');
    $this->addFieldMapping('status', 'status');
    $this->addFieldMapping('created', 'created');
    $this->addFieldMapping('changed', 'changed');
    $this->addFieldMapping('comment', 'comment');
    $this->addFieldMapping('promote', 'promote');
    // TODO: add pathauto!

    // Map the custom fields.
    $this->addFieldMapping('c4m_body', 'body')
      ->arguments(array('format' => 'full_html'));
    $this->addFieldMapping('c4m_vocab_geo', 'taxonomy')
      ->sourceMigration('C4dMigrateImportVocabGeo')
      ->arguments(array('source_type' => 'tid'));


    // Meta data!
    // TODO: migrate users needs to be done first!
    //$this->addFieldMapping('uid', 'uid')
    //     ->sourceMigration('C4dMigrateImportUsers');


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

    // Unserialize data
    $row->field_links = unserialize($row->field_links);
    $row->field_documents = unserialize($row->field_documents);
    $row->field_group_suggestions = unserialize($row->field_group_suggestions);
    $row->taxonomy = unserialize($row->taxonomy);

    // Prepare the file information (retrieve info from the files table).
    // TODO: extract the file info so migrate can move it and add it to the
    // new platform.

    return TRUE;
  }
}
