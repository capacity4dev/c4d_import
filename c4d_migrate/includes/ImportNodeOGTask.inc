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
   * Mapping between indexes and String values from export.
   *
   * @var array
   */
  private $casePriorities = array(
    1 => 'Low',
    2 => 'Normal',
    3 => 'High',
  );

  /**
   * Mapping between indexes and String values from export.
   *
   * @var array
   */
  private $caseStatuses = array(
    -1 => 'Not Started',
    0  => 'Ongoing',
    1  => 'Completed',
  );

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

    $this->addFieldMapping('c4m_task_assigned_to', 'assigned_to')
         ->sourceMigration('C4dMigrateImportUsers');

    $this->addFieldMapping('c4m_related_tasklist', 'tasklist')
         ->sourceMigration('C4dMigrateImportNodeOGTasklist');

    $this->addFieldMapping('c4m_task_priority', 'case_priority');

    $this->addFieldMapping('c4m_task_status', 'case_status');
  }

  /**
   * {@inheritdoc}
   */
  public function prepareRow($row) {
    parent::prepareRow($row);

    $row->case_priority = array_search($row->case_priority, $this->casePriorities);
    $row->case_status   = array_search($row->case_status, $this->caseStatuses);

    $row->field_ref_document = unserialize($row->field_ref_document);

    $row->body = $this->prepareHtml($row->body);
  }
}
