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
    $this->addFileFieldMapping('c4m_media', 'field_picture');
    // Other fields are not described yet for the Drupal 7 Content Type.

    $this->addFieldMapping('c4m_vocab_topic_mapping', 'taxonomy')
      ->arguments(array('source_type' => 'tid'));
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

    $destination = c4m_vocab_topic_get_mapping_by_source(trim($row->title));
    $c4m_vocab_topic = array();

    if (!empty($destination)) {
      foreach ($destination as $key => $topic) {
        $c4m_vocab_topic[] = $topic->tid;
      }
      $row->taxonomy = $c4m_vocab_topic;
    }

    return TRUE;
  }

  /**
   * Create stub Topics (when creating users), who will be replaced afterwards.
   *
   * @param Migration $migration
   *   The migration that is being done to create this stub.
   * @param array     $source_id
   *   Array of strings with the source id(s).
   *
   * @return array|bool
   *   Array with topic node id if term created, else FALSE.
   */
  public function createStub(Migration $migration, array $source_id) {
    $node                         = new stdClass();
    $node->title                  = t('Stub for @id', array('@id' => $source_id[0]));
    $node->body[LANGUAGE_NONE][0] = t('Stub topic');
    $node->type                   = $this->destination->getBundle();
    $node->uid                    = 1;
    $node->status                 = 0;
    $node->revision               = FALSE;
    node_save($node);
    if (isset($node->nid)) {
      return array($node->nid);
    }
    else {
      return FALSE;
    }
  }
}
