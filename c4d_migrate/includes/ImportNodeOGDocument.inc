<?php
/**
 * @file
 * Migrate class to import the Document group nodes.
 */

/**
 * Class to import the Document group nodes from the Drupal 6 platform.
 */
class C4dMigrateImportNodeOGDocument extends C4dMigrateImportBaseNodeOG {
  /**
   * Configure the object during creation.
   *
   * @param object $group
   *   The migrate Group object.
   */
  public function __construct($group = NULL) {
    $this->description = t('Import Document group nodes.');
    $this->setSourceTable('c4d_export_node_document');
    $this->setTargetBundle('document');

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

    // Map the picture field.
    $this->addFileFieldMapping('c4m_document', 'file_field');
  }

  /**
   * Prepare the row data loaded from the table.
   *
   * @param object $row
   *   The data from the table.
   *
   * @return bool
   *   Success or FALSE if it must be skipped.
   */
  public function prepareRow($row) {
    parent::prepareRow($row);

    // Prepare the picture data.
    $this->prepareFileInRow($row, 'file_field');

    $row->body = $this->prepareHtml($row->body);

    // Skip this row if it's an image.
    if ($this->isImageField($row, 'file_field')) {
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
