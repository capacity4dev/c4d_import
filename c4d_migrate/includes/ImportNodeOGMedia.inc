<?php
/**
 * @file
 * Migrate class to import the Media group nodes.
 */

/**
 * Class to import the Media group nodes from the Drupal 6 platform.
 *
 * Media refers to documents which are actually photos.
 */
class C4dMigrateImportNodeOGMedia extends C4dMigrateImportBaseNodeOG {
  /**
   * Configure the object during creation.
   *
   * @param object $group
   *   The migrate Group object.
   */
  public function __construct($group = NULL) {
    $this->description = t('Import Media group nodes.');
    $this->setSourceTable('c4d_export_node_document');
    $this->setTargetBundle('photo');

    $this->setCategoriesAndTags(TRUE);

    parent::__construct($group);

    $this->setPathManual(FALSE);
  }

  /**
   * {@inheritdoc}
   */
  public function addCustomFieldMapping() {
    // Map the custom fields.
    $this->addFieldMapping('c4m_body', 'body')
      ->arguments(array('format' => 'filtered_html'));

    // Map the picture field.
    $this->addFileFieldMapping('c4m_media', 'file_field');
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

    // Prepare the picture data.
    $this->prepareFileInRow($row, 'file_field');
    
    // Skip this row if it's not an image.
    if (!$this->isImageField($row, 'file_field')) {
      return FALSE;
    }

    return TRUE;
  }

  /**
   * Calls parent complete function.
   */
  public function complete($entity, $row) {
    return parent::complete($entity, $row);
  }

}
