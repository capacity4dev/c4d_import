<?php
/**
 * @file
 * Migrate class to import the Document comments.
 */

/**
 * Class to import the Document comments from the Drupal 6 platform.
 */
class C4dMigrateImportCommentOGDocument extends C4dMigrateImportBaseCommentOg {

  /**
   * Configure the object during creation.
   */
  public function __construct($group = NULL) {
    $this->description = t('Import Document comments.');

    $this->setNodeMigration('C4dMigrateImportNodeOGDocument');
    $this->setCommentMigration(__CLASS__);

    parent::__construct('document', 'document', 'c4d_export_node_document', $group);
  }
}
