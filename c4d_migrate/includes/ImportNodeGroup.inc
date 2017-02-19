<?php
/**
 * @file
 * Migrate class to import the Group nodes.
 */

/**
 * Class to import the Group nodes from the Drupal 6 platform.
 */
class C4dMigrateImportNodeGroup extends C4dMigrateImportBaseGroup {

  /**
   * Configure the object during creation.
   *
   * @param object $group
   *   The migrate Group object.
   */
  public function __construct($group = NULL) {
    $this->description = t('Import Group nodes.');
    $this->setSourceTable('c4d_export_node_group');
    $this->setTargetBundle('group');

    parent::__construct($group);
  }

  /**
   * {@inheritdoc}
   */
  public function addCustomFieldMapping() {
    // Map the access (public/private).
    $this->addFieldMapping('group_access', 'og_private');
    // Map the custom fields.
    $this->addFieldMapping('c4m_body', 'field_group_description')
      ->defaultValue('')
      ->arguments(array('format' => 'filtered_html'));
    $this->addFieldMapping('c4m_body:summary', 'og_description');
  }

  /**
   * Code to execute after the last row has been imported.
   */
  public function postImport() {
    parent::postImport();

    drupal_flush_all_caches();
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
    $row->field_group_description = $this->prepareHtml($row->field_group_description);

    if (empty($row->field_group_description)) {
      // No input filter is available when NULL is entered.
      // Even the defaultValue() is ignored on NULL.
      $row->field_group_description = '';
    }

    return TRUE;
  }

  /**
   * Review a data row after fetch, returning FALSE to skip it.
   *
   * @param object $row
   *   The data row we are importing.
   *
   * @return bool
   *   Successfully completed or not.
   */
  public function complete($entity, $row) {
    $this->savePluggableNodeAccess($entity, $row);

    parent::complete($entity, $row);
    return TRUE;
  }

}
