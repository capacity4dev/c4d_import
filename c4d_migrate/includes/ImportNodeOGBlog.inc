<?php
/**
 * @file
 * Migrate class to import the Blog group nodes.
 */

/**
 * Class to import the Blog group nodes from the Drupal 6 platform.
 */
class C4dMigrateImportNodeOGBlog extends C4dMigrateImportBaseNodeOG {
  /**
   * Configure the object during creation.
   *
   * @param MigrateGroup $group
   *   The migrate Group object.
   */
  public function __construct($group = NULL) {
    $this->description = t('Import Blog group nodes.');
    $this->setSourceTable('c4d_export_node_blog');
    $this->setTargetBundle('blog');

    $this->dependencies[] = 'C4dMigrateImportNodeOGDocument';

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
    parent::prepareRow($row);

    $row->body = $this->prepareHtml($row->body);
    $row->documents = unserialize($row->documents);

    return TRUE;
  }
}
