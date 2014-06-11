<?php
/**
 * @file
 * Migrate class to import the Blog comments.
 */

/**
 * Class to import the Blog comments from the Drupal 6 platform.
 */
class C4dMigrateImportCommentOGBlog extends C4dMigrateImportBaseComment {

  /**
   * Configure the object during creation.
   */
  public function __construct($group = NULL) {
    $this->description = t('Import Blog comments.');

    $this->setNodeMigration('C4dMigrateImportNodeOGBlog');
    $this->setCommentMigration(__CLASS__);

    parent::__construct('blog', 'c4d_export_node_blog', $group);
  }
}
