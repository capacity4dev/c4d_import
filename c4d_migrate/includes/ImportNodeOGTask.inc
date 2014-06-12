<?php
/**
 * @file
 * Migrate class to import the Task group nodes.
 */

/**
 * Class to import the Task group nodes from the Drupal 6 platform.
 */
class C4dMigrateImportNodeOGTask extends C4dMigrateImportBaseNodeOG {
  /**
   * Configure the object during creation.
   *
   * @param MigrateGroup $group
   *   The migrate Group object.
   */
  public function __construct($group = NULL) {
    $this->description = t('Import Task group nodes.');
    $this->setSourceTable('c4d_export_node_task');
    $this->setTargetBundle('task');

    $this->setCategoriesAndTags(FALSE);

    parent::__construct($group);
  }

  /**
   * {@inheritdoc}
   */
  public function addCustomFieldMapping() {
    // Map the custom fields.
    $this->addFieldMapping('c4m_body', 'body')
         ->arguments(array('format' => 'full_html'));

    $this->addFieldMapping('c4m_date', 'field_task_deadline');

    $this->addFieldMapping('c4m_related_document', 'field_ref_document')
         ->sourceMigration('C4dMigrateImportNodeOGDocument');

    $this->addFieldMapping('c4m_related_user', 'tasklist')
         ->sourceMigration('C4dMigrateImportUsers');

    $this->addFieldMapping('c4m_related_tasklist', 'assigned_to')
         ->sourceMigration('C4dMigrateImportNodeOGTasklist');

    $this->addFieldMapping('c4m_task_priority', 'case_priority');

    $this->addFieldMapping('c4m_task_status', 'case_status');
  }

  /**
   * {@inheritdoc}
   */
  public function prepareRow($row) {
    parent::prepareRow($row);

    $row->field_project_status   = ($row->field_project_status == 'open') ? '1' : '0';
    $row->field_project_managers = unserialize($row->field_project_managers);
    $row->field_ref_document     = unserialize($row->field_ref_document);
  }
}
