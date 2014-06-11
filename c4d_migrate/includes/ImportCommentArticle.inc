<?php
/**
 * @file
 * Migrate class to import the Article comments.
 */

/**
 * Class to import the Article comments from the Drupal 6 platform.
 */
class C4dMigrateImportCommentArticle extends C4dMigrateImportBaseComment {

  /**
   * Configure the object during creation.
   */
  public function __construct($group = NULL) {
    $this->description = t('Import Article comments.');

    $this->setNodeMigration('C4dMigrateImportNodeArticle');
    $this->setCommentMigration(__CLASS__);

    parent::__construct('article', 'article', 'c4d_export_node_article', $group);
  }
}
