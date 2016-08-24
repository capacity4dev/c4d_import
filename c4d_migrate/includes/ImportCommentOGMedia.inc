<?php
/**
 * @file
 * Migrate class to import the Media comments.
 */

/**
 * Class to import the Media comments from the Drupal 6 platform.
 */
class C4dMigrateImportCommentOGMedia extends C4dMigrateImportBaseCommentOg {

  /**
   * Configure the object during creation.
   */
  public function __construct($group = NULL) {
    $this->description = t('Import Media comments.');

    $this->setNodeMigration('C4dMigrateImportNodeOGMedia');
    $this->setCommentMigration(__CLASS__);

    parent::__construct('document', 'photo', 'c4d_export_node_document', $group);
  }

}
