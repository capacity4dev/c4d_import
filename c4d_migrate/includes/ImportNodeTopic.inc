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
         ->arguments(array('format' => 'filtered_html'));
    $this->addFieldMapping('c4m_body:summary', 'field_brief');

    // Map the picture field.
    $this->addFileFieldMapping('c4m_media', 'field_picture', FILE_EXISTS_REPLACE);
    $this->addFileFieldMapping('c4m_banner', 'field_picture_topics_page', FILE_EXISTS_REPLACE);

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
    $this->prepareFileInRow($row, 'field_picture_topics_page');

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

    // Also add the topic group variable to the groups mapping.
    if (isset($row->field_topic_group) && is_numeric($row->field_topic_group)) {
      if (!in_array($row->field_topic_group, $row->field_group_suggestions)) {
        $row->field_group_suggestions[] = $row->field_topic_group;
      }
    }

    $row->field_topic_articles = unserialize($row->field_topic_articles);

    return TRUE;
  }

  /**
   * Review a data row after fetch, returning FALSE to skip it.
   *
   * @param object $node
   *   The drupal node entity object.
   * @param object $row
   *   The source row object.
   */
  public function prepare($node, $row) {
    $query = $this->createSelect('c4d_export_node_topic', 't');
    $query->addField('t', 'nid');
    // Search for duplicate paths.
    $query->condition('path', $row->path, '=');
    // The possible "parental" topic node always has a lower node ID than the
    // derived topic nodes, so we just set a condition on node IDs which are
    // lower than the current one.
    // This way we also exclude querying the current node, so no need for:
    //   $query->condition('nid', $row->nid, '<>');
    $query->condition('nid', $row->nid, '<');
    $results = $query->execute();

    if ($results->rowCount()) {
      // Oops, we found a duplicate path, so disable this one.
      $node->path = NULL;
    }
  }

}
