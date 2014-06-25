<?php
/**
 * @file
 * Migrate class to import the Tasklist group nodes.
 */

/**
 * Class to import the Tasklist group nodes from the Drupal 6 platform.
 */
class C4dMigrateImportNodeOGTasklist extends C4dMigrateImportBaseNodeOG {
  /**
   * Configure the object during creation.
   *
   * @param MigrateGroup $group
   *   The migrate Group object.
   */
  public function __construct($group = NULL) {
    $this->description = t('Import Tasklist group nodes.');
    $this->setSourceTable('c4d_export_node_project');
    $this->setTargetBundle('tasklist');

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

    $this->addFieldMapping('c4m_tasklist_status', 'field_project_status');

    $this->addFieldMapping('c4m_related_user', 'field_project_managers')
         ->sourceMigration('C4dMigrateImportUsers');

    $this->addFieldMapping('c4m_related_document', 'field_ref_document')
         ->sourceMigration('C4dMigrateImportNodeOGDocument');
  }

  /**
   * {@inheritdoc}
   */
  public function prepareRow($row) {
    parent::prepareRow($row);

    $row->field_project_status   = ($row->field_project_status == 'open') ? '1' : '0';
    $row->field_project_managers = unserialize($row->field_project_managers);
    $row->field_ref_document     = unserialize($row->field_ref_document);

    $row->body = $this->prepareHtml($row->body);
  }
}
