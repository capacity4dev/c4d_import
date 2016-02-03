<?php
/**
 * @file
 * Migrate class to import the OG Categories terms.
 */

/**
 * Class C4dMigrateImportVocabOGCategories.
 */
class C4dMigrateImportVocabOGCategories extends C4dMigrateImportBaseVocabTerms {
  /**
   * Content creations need to be run before this one.
   *
   * @var array
   */
  public $dependencies = array(
    'C4dMigrateImportNodeGroup',
    'C4dMigrateImportNodeProject',
  );

  /**
   * Configure the object during creation.
   */
  public function __construct() {
    parent::__construct(
          MigrateGroup::getInstance(C4D_MIGRATE_MIGRATION_GROUP)
    );

    $this->description = t('Import OG Categories vocabulary terms.');

    // Define the source.
    $query        = $this->createSelect('c4d_export_vocabulary_og_categories', 'og_cats')
                         ->fields('og_cats')
                         ->orderBy('parent, tid');
    $this->source = new MigrateSourceSQL(
      $query,
      array(),
      NULL,
      array('map_joinable' => FALSE)
    );

    // Import into the og vocabularies table {c4m_vocab_category_<GID>}.
    $term_options['allow_duplicate_terms'] = TRUE;
    $destination_handler                   = new MigrateDestinationTerm(
      'c4m_vocab_category',
      $term_options
    );

    // Create a map object for tracking the
    // relationships between source & destination rows.
    $key               = array(
      'tid' => array(
        'type'     => 'int',
        'not null' => TRUE,
        'unsigned' => TRUE,
      ),
    );
    $this->map         = new MigrateSQLMap(
      $this->machineName,
      $key,
      $destination_handler->getKeySchema()
    );
    $this->destination = $destination_handler;

    $this->addFieldMapping('vocabulary_machine_name', 'vocabulary_name_mapped');

    // Map the fields.
    $this->addFieldMapping('name', 'name');
    $this->addFieldMapping('description', 'description');
    $this->addFieldMapping('weight', 'weight');
    // Use the migration mapping to link child with parent.
    $this->addFieldMapping('parent', 'parent')
         ->sourceMigration($this->machineName);
  }

  /**
   * Review a data row after fetch, returning FALSE to skip it.
   *
   * @param object $row
   *   The data row we are importing.
   */
  public function prepareRow($row) {
    parent::prepareRow($row);

    $vocab_name = 'c4m_vocab_category_' . $this->getMappedGroupGid($row->type, $row->gid);

    $row->vocabulary_name_mapped = $vocab_name;
  }

}
