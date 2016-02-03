<?php
/**
 * @file
 * Migrate class to import the Task comments.
 */

/**
 * Class to import the Task comments from the Drupal 6 platform.
 */
class C4dMigrateImportCommentOGTask extends C4dMigrateImportBaseCommentOg {

  /**
   * Configure the object during creation.
   */
  public function __construct($group = NULL) {
    $this->description = t('Import Task comments.');

    $this->setNodeMigration('C4dMigrateImportNodeOGTask');
    $this->setCommentMigration(__CLASS__);

    parent::__construct('task', 'task', 'c4d_export_node_task', $group);
  }

}
