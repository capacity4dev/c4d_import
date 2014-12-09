<?php
/**
 * @file
 * Migrate class to import the Discussion comments.
 */

/**
 * Class to import the Blog comments from the Drupal 6 platform
 * as Discussion comments.
 */
class C4dMigrateImportCommentOGDiscussion extends C4dMigrateImportBaseCommentOg {

  /**
   * Configure the object during creation.
   */
  public function __construct($group = NULL) {
    $this->description = t('Import Discussion comments.');

    $this->setNodeMigration('C4dMigrateImportNodeOGDiscussion');
    $this->setCommentMigration(__CLASS__);

    parent::__construct('blog', 'discussion', 'c4d_export_node_blog', $group);
  }
}
