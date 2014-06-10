<?php
/**
 * @file
 * Migrate class to import the Photoalbum group nodes.
 */

/**
 * Class to import the Photoalbum group nodes from the Drupal 6 platform.
 */
class C4dMigrateImportNodeOGPhotoalbum extends C4dMigrateImportBaseNodeOG {
  /**
   * Configure the object during creation.
   *
   * @param MigrateGroup $group
   *   The migrate Group object.
   */
  public function __construct($group = NULL) {
    $this->description = t('Import Photoalbum group nodes.');
    $this->setSourceTable('c4d_export_node_node_gallery_gallery');
    $this->setTargetBundle('photoalbum');

    $this->setCategoriesAndTags(TRUE);

    parent::__construct($group);
  }

  /**
   * {@inheritdoc}
   */
  public function addCustomFieldMapping() {
    // Map the custom fields.
    $this->addFieldMapping('c4m_body', 'body')
         ->arguments(array('format' => 'full_html'));
  }
}
