<?php
/**
 * @file
 * Base class to import data into nodes.
 */

/**
 * Base class to import data into nodes.
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
   * General initialization of a Migration object.
   *
   * This implements the common functionality to import nodes from a flat table.
   *
   * @param MigrateGroup $group
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
    $this->addDefaultFieldMapping();
    $this->addCustomFieldMapping();
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

    // TODO: add user mapping!


    // TODO: add pathauto!
  }

  /**
   * Set the source table name.
   *
   * @param string $table
   *
   * @return C4dMigrateImportBaseNode
   */
  public function setSourceTable($table) {
    $this->sourceTable = $table;
    return $this;
  }

  /**
   * Get the source table name.
   *
   * @return string
   */
  public function getSourceTable() {
    return $this->sourceTable;
  }

  /**
   * Set the bundle we want to import to.
   *
   * @param string $bundle
   *
   * @return C4dMigrateImportBaseNode
   */
  public function setTargetBundle($bundle) {
    $this->targetBundle = $bundle;
    return $this;
  }

  /**
   * Get the bundle we want to import to.
   *
   * @return string
   */
  public function getTargetBundle() {
    return $this->targetBundle;
  }
}
