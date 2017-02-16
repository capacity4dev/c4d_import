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
   * @param object $group
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
         ->arguments(array('format' => 'filtered_html'));

    $this->removeFieldMapping('comment');
    $this->addFieldMapping('comment')->defaultValue(COMMENT_NODE_CLOSED);
  }

  /**
   * Prepare the row data loaded from the table.
   *
   * @param object $row
   *   The data from the table.
   *
   * @return bool
   *   Success.
   */
  public function prepareRow($row) {
    parent::prepareRow($row);
    $row->body = $this->prepareHtml($row->body);
    return TRUE;
  }

  /**
   * Calls parent complete function.
   */
  public function complete($entity, $row) {
    return parent::complete($entity, $row);
  }

}
