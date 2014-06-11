<?php
/**
 * @file
 * Migrate class to import the Event comments.
 */

/**
 * Class to import the Event comments from the Drupal 6 platform.
 */
class C4dMigrateImportCommentOGEvent extends C4dMigrateImportBaseComment {

  /**
   * Configure the object during creation.
   */
  public function __construct($group = NULL) {
    $this->description = t('Import Event comments.');

    $this->setNodeMigration('C4dMigrateImportNodeOGEvent');
    $this->setCommentMigration(__CLASS__);

    parent::__construct('event', 'c4d_export_node_event', $group);
  }
}
