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
   *
   * @param object|NULL $group
   */
  public function __construct($group = NULL) {
    $this->description = t('Import Article comments.');

    $this->setNodeMigration('C4dMigrateImportNodeArticle');
    $this->setCommentMigration(__CLASS__);

    parent::__construct('article', 'article', 'c4d_export_node_article', $group);
  }

  /**
   * Actions after the data is saved in the database.
   *
   * @param object $entity
   *   The entity that is saved into the platform.
   * @param object $row
   *   The data row we are importing.
   */
  public function complete($entity, $row) {
    // Make sure the messages have the proper timestamp.
    $this->messageInsertSave($entity, $row);
  }
}
