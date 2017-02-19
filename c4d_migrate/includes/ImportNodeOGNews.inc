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
class C4dMigrateImportNodeOGNews extends C4dMigrateImportBaseNodeOG {
  /**
   * Configure the object during creation.
   *
   * @param object $group
   *   The migrate Group object.
   */
  public function __construct($group = NULL) {
    $this->description = t('Import News project nodes.');
    $this->setSourceTable('c4d_export_node_blog');
    $this->setTargetBundle('news');

    $this->dependencies[] = 'C4dMigrateImportNodeOGDocument';

    $this->setCategoriesAndTags(TRUE);

    parent::__construct($group);
  }

  /**
   * {@inheritdoc}
   *
   */
  public function addCustomFieldMapping() {
    // Map the custom fields.
    $this->addFieldMapping('c4m_body', 'body')
      ->arguments(array('format' => 'filtered_html'));

    $this->addFieldMapping('c4m_related_document', 'documents')
      ->sourceMigration('C4dMigrateImportNodeOGDocument');
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
    $groups = unserialize($row->og_groups);

    foreach ($groups as $gid => $group_type) {
      if ($group_type == 'group') {
        // No news in groups.
        return FALSE;
      }
    }

    parent::prepareRow($row);

    $row->body = $this->prepareHtml($row->body);
    $row->documents = unserialize($row->documents);

    return TRUE;
  }

  /**
   * Calls parent complete function.
   */
  public function complete($entity, $row) {
    return parent::complete($entity, $row);
  }

}
