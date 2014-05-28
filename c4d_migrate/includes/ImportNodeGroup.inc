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
   * @param MigrateGroup $group
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
    // Map the custom fields.
    $this->addFieldMapping('c4m_body', 'field_group_description')
         ->arguments(array('format' => 'full_html'));
    $this->addFieldMapping('c4m_body:summary', 'og_description');

  }
}
