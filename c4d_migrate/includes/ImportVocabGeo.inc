<?php
/**
 * @file
 *
 */

/**
 * Migrate class to import the Geographical vocabulary.
 */
class C4dMigrateImportVocabGeo extends C4dMigrateImportBase {
  /**
   * Constructor to setup the mapping between the old DB and the new entities.
   */
  public function __construct() {
    parent parent::__construct();

    $this->description = t('Import the D6 geographical terms.');

    $DB = Database::getConnection('default', C4D_MIGRATE_DATABASE);

    $query = $DB->select('c4d_export_node_geo_region', 'georegion');
    $this->source = new MigrateSourceSQL($query);


    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'source_id' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
          'description' => 'Source ID',
        )
      ),
      MigrateDestinationNode::getKeySchema(),
      C4D_MIGRATE_DATABASE
    );
  }
}