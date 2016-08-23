<?php
/**
 * @file
 * Migrate class to import the Topic nodes.
 */

/**
 * Class to import the Topic nodes from the Drupal 6 platform.
 */
class C4dMigrateImportNodeTopic extends C4dMigrateImportBaseNode {
  public $entityType = 'node';
  public $bundle = 'topic';

  /**
   * Content creations that should run before this one.
   *
   * @var array
   */
  public $dependencies = array(
    'C4dMigrateImportUsers',
    'C4dMigrateCreateCSVTermTopic',
    'C4dMigrateImportNodeArticle',
    'C4dMigrateImportNodeGroup',
  );

  /**
   * Configure the object during creation.
   */
  public function __construct($group = NULL) {
    $this->description = t('Import Topic nodes.');
    $this->setSourceTable('c4d_export_node_topic');
    $this->setTargetBundle('topic');

    parent::__construct($group);
  }

  /**
   * {@inheritdoc}
   */
  public function addCustomFieldMapping() {
    // Map the custom fields.
    $this->addFieldMapping('c4m_body', 'body')
         ->arguments(array('format' => 'full_html'));

    // Map the picture field.
    $this->addFileFieldMapping('c4m_media', 'field_picture', FILE_EXISTS_REPLACE);

    // Other fields are not described yet for the Drupal 7 Content Type.
    $this->addFieldMapping('c4m_vocab_topic_mapping', 'taxonomy')
      ->arguments(array('source_type' => 'tid'));

    $this->addFieldMapping('c4m_related_group_unlimited', 'field_group_suggestions')
      ->sourceMigration('C4dMigrateImportNodeGroup');

    $this->addFieldMapping('c4m_related_articles', 'field_topic_articles')
      ->sourceMigration('C4dMigrateImportNodeArticle');
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

    // Prepare the picture data.
    $this->prepareFileInRow($row, 'field_picture');

    $row->body = $this->prepareHtml($row->body);

    $destination = c4m_vocab_topic_get_mapping_by_destination(trim($row->title));
    $c4m_vocab_topic = array();

    if (!empty($destination)) {
      foreach ($destination as $key => $topic) {
        $c4m_vocab_topic[] = $topic->tid;
      }
      $row->taxonomy = $c4m_vocab_topic;
    }

    if ($row->field_group_suggestion_type == 1) {
      $row->field_group_suggestions = unserialize($row->field_group_suggestions);
    }
    else {
      // Might be some leftover groups, but the type might be set to "auto".
      $row->field_group_suggestions = array();
    }

    $row->field_topic_articles = unserialize($row->field_topic_articles);

    return TRUE;
  }

}
