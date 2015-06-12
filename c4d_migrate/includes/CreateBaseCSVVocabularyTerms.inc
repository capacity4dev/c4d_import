<?php
/**
 * @file
 * Base class to create Entities from CSV.
 */


/**
 * Class C4dMigrateCreateBaseCSV.
 */
abstract class C4dMigrateCreateBaseCSVVocabularyTerms extends C4dMigrateImportBase {
  /**
   * The node bundle we want to import to.
   *
   * @var string
   */
  private $targetBundle;
  /**
   * General initialization of a Migration object.
   *
   * This implements the common functionality to import taxonomy terms via CSV.
   *
   * @param MigrateGroup $group
   *   The migrate Group object.
   */
  public function __construct() {
    parent::__construct(
      MigrateGroup::getInstance(C4D_MIGRATE_MIGRATION_GROUP)
    );

    $this->description = t('Import Vocabulary terms.');

    // Override to be a CSV source.
    $csv_path = drupal_get_path('module', 'c4d_migrate') . '/includes/csv';
    $this->source = new MigrateSourceCSV(
      $csv_path . '/taxonomy_term/' . $this->getTargetBundle() . '.csv',
      $this->csvColumns,
      array('header_rows' => 1)
    );

    // Set the destination node bundle to import the data to.
    $destination_handler = new MigrateDestinationTerm($this->getTargetBundle());

    // Add map to track relationships between source & destination rows.
    $key = array(
      'id' => array(
        'type'     => 'varchar',
        'length'   => 255,
        'not null' => TRUE,
      ),
    );
    $this->map = new MigrateSQLMap(
      $this->machineName,
      $key,
      $destination_handler->getKeySchema()
    );

    $this->destination = $destination_handler;

    // Map the fields.
    $this->addDefaultFieldMapping();
    $this->addCustomFieldMapping();
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
   * Add field mapping for the non-common node fields.
   */
  public function addCustomFieldMapping() {
  }

  /**
   * Add the mapping for the common node fields.
   */
  public function addDefaultFieldMapping() {
    $this->addFieldMapping('name', 'title');
    $this->addFieldMapping('weight', 'weight');
    $this->addFieldMapping('description', 'description');
  }


}