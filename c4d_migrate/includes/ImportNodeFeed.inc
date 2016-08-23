<?php
/**
 * @file
 * Migrate class to import the Feed nodes.
 */

/**
 * Class to import the Feed nodes from the Drupal 6 platform.
 */
class C4dMigrateImportNodeFeed extends C4dMigrateImportBaseNode {
  /**
   * Content creations that should run before this one.
   *
   * @var array
   */
  public $dependencies
    = array(
      'C4dMigrateImportUsers',
      'C4dMigrateImportNodeArticle',
      'C4dMigrateImportNodeGroup',
      'C4dMigrateImportNodeTopic',
    );

  /**
   * Configure the object during creation.
   */
  public function __construct($group = NULL) {
    $this->description = t('Import Feed nodes.');
    $this->setSourceTable('c4d_export_node_feed');
    $this->setTargetBundle('feed');

    parent::__construct($group);
  }

  /**
   * {@inheritdoc}
   */
  public function addCustomFieldMapping() {
    // Map the custom fields.
    $this->addFieldMapping('c4m_body', 'body')
      ->arguments(array('format' => 'full_html'));

    $this->addFieldMapping('c4m_related_articles_unlimited', 'field_ref_related_articles')
      ->sourceMigration('C4dMigrateImportNodeArticle');

    $this->addFieldMapping('c4m_related_group_unlimited', 'field_ref_related_groups')
      ->sourceMigration('C4dMigrateImportNodeGroup');

    $this->addFieldMapping('c4m_related_topic', 'field_ref_related_topics')
      ->sourceMigration('C4dMigrateImportNodeTopic');
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

    // Unserialize data.
    $row->field_ref_related_articles = unserialize($row->field_ref_related_articles);
    $row->field_ref_related_groups = unserialize($row->field_ref_related_groups);
    $row->field_ref_related_topics = unserialize($row->field_ref_related_topics);

    return TRUE;
  }

}
