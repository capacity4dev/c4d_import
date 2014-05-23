<?php
/**
 * @file
 * Migrate class to import the Geographical terms.
 */

/**
 * Class C4dMigrateImportVocabGeo
 */
class C4dMigrateImportVocabGeo extends C4dMigrateImportBase {
  /**
   * Configure the object during creation.
   */
  public function __construct() {
    parent::__construct(
      MigrateGroup::getInstance(C4D_MIGRATE_MIGRATION_GROUP)
    );

    $this->description = t('Import Geographical vocabulary terms.');

    // Define the source.
    $query = $this->createSelect('c4d_export_vocabulary_regions_countries', 'geo')
                  ->fields('geo');
    $this->source = new MigrateSourceSQL($query);

    // Import into the c4m_vocab_geo table.
    $term_options['allow_duplicate_terms'] = TRUE;
    $destination_handler = new MigrateDestinationTerm(
      'c4m_vocab_geo',
      $term_options
    );

    // Create a map object for tracking the
    // relationships between source & destination rows.
    $key = array(
      'tid' => array(
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
    $this->addFieldMapping('name', 'name');
    $this->addFieldMapping('description', 'description');
    $this->addFieldMapping('weight', 'weight');
    // Use the migration mapping to link child with parent.
    $this->addFieldMapping('parent', 'parent')
         ->sourceMigration($this->machineName);
  }
}
