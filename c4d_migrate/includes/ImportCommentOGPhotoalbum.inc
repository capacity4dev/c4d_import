<?php
/**
 * @file
 * Migrate class to import the Photoalbum comments.
 */

/**
 * Class to import the Photoalbum comments from the Drupal 6 platform.
 */
class C4dMigrateImportCommentOGPhotoalbum extends C4dMigrateImportBaseCommentOg {

  /**
   * Configure the object during creation.
   */
  public function __construct($group = NULL) {
    $this->description = t('Import Photoalbum comments.');

    $this->setNodeMigration('C4dMigrateImportNodeOGPhotoalbum');
    $this->setCommentMigration(__CLASS__);

    parent::__construct('node_gallery_gallery', 'photoalbum', 'c4d_export_node_node_gallery_gallery', $group);
  }

}
