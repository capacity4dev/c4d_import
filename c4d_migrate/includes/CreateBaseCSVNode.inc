<?php
/**
 * @file
 * Base class to create Entities from CSV.
 */


/**
 * Class C4dMigrateCreateBaseCSV.
 */
abstract class C4dMigrateCreateBaseCSVNode extends C4dMigrateImportBaseNode {
  // Define source ID's so we can retrieve them later on to map them.
  // Needs to be.
  const C4D_MIGRATE_ORGANISATION_EC_ID = 0641547101;
  const C4D_MIGRATE_ORGANISATION_EU_ID = 0641547102;

  /**
   * General initialization of a Migration object.
   *
   * This implements the common functionality to import nodes from a flat table.
   *
   * @param MigrateGroup $group
   *   The migrate Group object.
   */
  public function __construct($group = NULL) {
    parent::__construct($group, TRUE);

    // Override to be a CSV source.
    $csv_path = drupal_get_path('module', 'c4d_migrate') . '/includes/csv';
    $this->source = new MigrateSourceCSV(
      $csv_path . '/node/' . $this->getTargetBundle() . '.csv',
      $this->csvColumns,
      array('header_rows' => 1)
    );

    // Set the destination node bundle to import the data to.
    $destination_handler = new MigrateDestinationNode($this->getTargetBundle());

    // Add map to track relationships between source & destination rows.
    $key = array(
      'id' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
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

}
