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

    $this->addFieldMapping('c4m_vocab_topic', 'topics')
      ->arguments(array('source_type' => 'tid'));

    $this->addFieldMapping('c4m_related_articles', 'field_ref_related_articles')
      ->sourceMigration('C4dMigrateImportNodeArticle');

    $this->addFieldMapping('c4m_related_group', 'field_ref_related_groups')
      ->sourceMigration('C4dMigrateImportNodeGroup');
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

    // Unserialize data.
    $topic_nids = unserialize($row->field_ref_related_topics);
    $topics = array();

    foreach ($topic_nids as $nid) {
      $title = $this->getNodeTitleFromNid($nid);

      $mapped = c4m_vocab_topic_get_mapping_by_source($title);

      foreach ($mapped as $term) {
        $topics[] = $term->tid;
      }
    }

    $row->topics = $topics;
    $row->field_ref_related_articles = unserialize($row->field_ref_related_articles);
    $row->field_ref_related_groups = unserialize($row->field_ref_related_groups);
    $row->body = $this->prepareHtml($row->body);

    return TRUE;
  }

}
