<?php
/**
 * @file
 * Migrate class to import the Minisite page comments.
 */

/**
 * Class to import the Minisite page comments from the Drupal 6 platform.
 */
class C4dMigrateImportCommentOGMinisite extends C4dMigrateImportBaseCommentOg {

  /**
   * Configure the object during creation.
   */
  public function __construct($group = NULL) {
    $this->description = t('Import Minisite page comments.');

    $this->setNodeMigration('C4dMigrateImportNodeOGMinisite');
    $this->setCommentMigration(__CLASS__);

    parent::__construct('minisite_page', 'page', 'c4d_export_node_minisite_page', $group);
  }
}
