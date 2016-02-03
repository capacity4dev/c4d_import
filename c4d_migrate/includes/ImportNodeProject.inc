<?php
/**
 * @file
 * Migrate class to import the Project nodes.
 */

/**
 * Class to import the Project nodes from the Drupal 6 platform.
 */
class C4dMigrateImportNodeProject extends C4dMigrateImportBaseGroup {

  /**
   * Configure the object during creation.
   *
   * @param object $project
   *   The migrate Project object.
   */
  public function __construct($project = NULL) {
    $this->dependencies[] = 'C4dMigrateImportNodeGroup';

    $this->description = t('Import Project nodes.');
    $this->setSourceTable('c4d_export_node_project_group');
    $this->setTargetBundle('project');

    parent::__construct($project);
  }

  /**
   * {@inheritdoc}
   */
  public function addCustomFieldMapping() {
    // Map the custom fields.
    $this->addFieldMapping('c4m_body', 'body')
         ->arguments(array('format' => 'full_html'));
    $this->addFieldMapping('c4m_body:summary', 'og_description');

    $this->addFieldMapping('c4m_related_group', 'field_ref_related_user_groups')
         ->sourceMigration('C4dMigrateImportNodeGroup');
    $this->addFieldMapping('c4m_related_projects', 'field_ref_related_projects')
         ->sourceMigration('C4dMigrateImportNodeProject');

    $this->addFieldMapping('c4m_partner', 'field_project_group_stakeholders');
    $this->addFieldMapping('c4m_source_of_funding', 'field_project_group_funding');
    $this->addFieldMapping('c4m_cris_decision_number', 'field_project_group_cris_dec');
    $this->addFieldMapping('c4m_cris_contract_number', 'field_project_group_cris_contract');

    $this->addFieldMapping('field_project_group_budget_amount', 'field_project_group_budget_amount');
    $this->addFieldMapping('field_project_group_budget_currency', 'field_project_group_budget_currency');

    $this->addFieldMapping('c4m_date_end', 'field_project_group_dates_start');
    $this->addFieldMapping('c4m_date_end:to', 'field_project_group_dates_end');
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

    $row->field_project_group_stakeholders = unserialize($row->field_project_group_stakeholders);
    $row->field_project_group_funding      = unserialize($row->field_project_group_funding);
    $row->field_project_group_cris_dec     = unserialize($row->field_project_group_cris_dec);
    $row->field_project_group_contract     = unserialize($row->field_project_group_contract);
    $row->field_ref_related_user_groups    = unserialize($row->field_ref_related_user_groups);
    $row->field_ref_related_projects       = unserialize($row->field_ref_related_projects);

    // Prepare the picture data.
    $this->prepareFileInRow($row, 'field_og_image');

    $row->body = $this->prepareHtml($row->body);

    return TRUE;
  }

  /**
   * Review a data row after fetch, returning FALSE to skip it.
   *
   * @param object $row
   *   The data row we are importing.
   *
   * @return bool
   *   Successfully finished complete or not.
   */
  public function complete($node, $row) {
    parent::complete($node, $row);

    if ($row->field_project_group_budget_amount == 0.00 || empty($row->field_project_group_budget_amount)) {
      return FALSE;
    }
    $node->c4m_money = array(
      LANGUAGE_NONE => array(
        array(
          'amount'   => $row->field_project_group_budget_amount,
          'currency' => $row->field_project_group_budget_currency,
        ),
      ),
    );
    $this->nodeSave($node, $row->changed);

    return TRUE;
  }

  /**
   * Code to execute after the last row has been imported.
   */
  public function postImport() {
    parent::postImport();

    drupal_flush_all_caches();
  }

  /**
   * Create stub Topics (when creating users), who will be replaced afterwards.
   *
   * @param Migration $migration
   *   The migration that is being done to create this stub.
   * @param array $source_id
   *   Array of strings with the source id(s).
   *
   * @return array|bool
   *   Array with topic node id if term created, else FALSE.
   */
  public function createStub(Migration $migration, array $source_id) {
    $node                                              = new stdClass();
    $node->title                                       = t('Stub for @id', array('@id' => $source_id[0]));
    $node->body[LANGUAGE_NONE][0]                      = t('Stub project');
    $node->type                                        = $this->destination->getBundle();
    $node->uid                                         = 1;
    $node->{OG_GROUP_FIELD}[LANGUAGE_NONE][0]['value'] = 1;
    $node->status                                      = 0;
    $node->revision                                    = FALSE;
    node_save($node);
    if (isset($node->nid)) {
      return array($node->nid);
    }
    else {
      return FALSE;
    }
  }

}
