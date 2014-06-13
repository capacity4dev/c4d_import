<?php
/**
 * @file
 * Class to import data into comments.
 */

/**
 * Class to import data into comments.
 *
 * This class has the common functionality to import comments:
 *   - It creates the query on the source table based on the sourceTable.
 *   - It creates the migration source object.
 *   - It creates the migration target object based on the targetBundle.
 *   - It creates the migration mapping object.
 *   - It defines the default mapping for comments.
 */
abstract class C4dMigrateImportBaseComment extends C4dMigrateImportBase {
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
   * Class of the node migrations (to map node ids).
   *
   * @var string
   */
  private $nodeMigration;

  /**
   * Subclass of the comment migrations (to map parent ids).
   *
   * @var string
   */
  private $commentMigration;

  /**
   * Content creations that should run before this one.
   *
   * @var array
   */
  public $dependencies;

  /**
   * General initialization of a Migration object.
   *
   * This implements the common functionality to import nodes from a flat table.
   *
   * @param MigrateGroup $group
   *   The migrate Group object.
   */
  public function __construct($source_node_type, $destination_node_type, $base_node_export_table, $group = NULL) {
    $this->description = t('Import Comments.');
    $this->setSourceTable('c4d_export_comment');
    $this->setTargetBundle($destination_node_type);

    $this->dependencies = array(
      $this->getNodeMigration(),
    );

    parent::__construct($group);

    // Define the source table to import the data from.
    $query = $this->createSelect($this->getSourceTable(), 'c')
                  ->fields('c');
    // Lots of comments were made on nodes that belong to groups that were
    // deleted. The node id still exists but is not migrated (filter out nodes
    // that are orphans during migration). So we map comments vs the exported
    // table of their nodes.
    $query->rightJoin($base_node_export_table, 'n', 'n.nid = c.nid');
    $query->orderBy('pid, cid')
          ->condition('node_type', $source_node_type, '=');
    $this->source = new MigrateSourceSQL(
      $query
    );

    // Set the destination node bundle to import the data to.
    $destination_handler = new MigrateDestinationComment('comment_node_' . $destination_node_type);

    // Add map to track relationships between source & destination rows.
    $key               = array(
      'cid' => array(
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

    // Map the fields.
    $this->addDefaultFieldMapping();
  }

  /**
   * Add the mapping for the common node fields.
   */
  public function addDefaultFieldMapping() {
    $this->addFieldMapping('pid', 'pid')
         ->sourceMigration($this->getCommentMigration());

    $this->addFieldMapping('nid', 'nid')
         ->sourceMigration($this->getNodeMigration());

    $this->addFieldMapping('uid', 'uid')
         ->sourceMigration('C4dMigrateImportUsers');

    $this->addSimpleMappings(array('name', 'subject', 'hostname'));

    $this->addFieldMapping('created', 'timestamp');

    $this->addFieldMapping('comment_body', 'comment')
         ->arguments(array('format' => 'full_html'));

    $this->addFieldMapping('status', 'status')
         ->defaultValue(COMMENT_PUBLISHED);
  }

  /**
   * {@inheritdoc}
   */
  public function prepareRow($row) {
    // Drupal 6 published: status = 0
    // Drupal 7 published: status = 1, we need to swap this around.
    $row->status = ($row->status == 0) ? 1 : 0;
  }

  /**
   * Set the source table name.
   *
   * @param string $table
   *   The source table name.
   *
   * @return C4dMigrateImportBaseNode
   *   Support for fluent interface.
   */
  public function setSourceTable($table) {
    $this->sourceTable = $table;

    return $this;
  }

  /**
   * Get the source table name.
   *
   * @return string
   *   The name of the source table.
   */
  public function getSourceTable() {
    return $this->sourceTable;
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
   * Set the current node migration class (needed for node mapping).
   *
   * @param string $node_migration
   *   Class name of the node migration class.
   */
  public function setNodeMigration($node_migration) {
    $this->nodeMigration = $node_migration;
  }

  /**
   * Get the current node migration class (needed for node mapping).
   *
   * @return string
   *   Class name of the node migration class.
   */
  public function getNodeMigration() {
    return $this->nodeMigration;
  }

  /**
   * Set the current comment migration class (needed for mapping of parents).
   *
   * @param string $comment_migration
   *   Class name of base commentMigration.
   */
  public function setCommentMigration($comment_migration) {
    $this->commentMigration = $comment_migration;
  }

  /**
   * Get the current comment migration class (needed for mapping of parents).
   *
   * @return string
   *   Class name of base commentMigration.
   */
  public function getCommentMigration() {
    return $this->commentMigration;
  }
}
