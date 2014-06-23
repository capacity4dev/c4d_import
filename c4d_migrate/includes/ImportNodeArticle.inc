<?php
/**
 * @file
 * Migrate class to import the Article nodes.
 */

/**
 * Class to import the Article nodes from the Drupal 6 platform.
 */
class C4dMigrateImportNodeArticle extends C4dMigrateImportBaseNode {
  /**
   * Content creations that should run before this one.
   *
   * @var array
   */
  public $dependencies = array(
    'C4dMigrateImportUsers',
    'C4dMigrateImportNodeTopic',
  );

  /**
   * Configure the object during creation.
   */
  public function __construct($group = NULL) {
    $this->description = t('Import Article nodes.');
    $this->setSourceTable('c4d_export_node_article');
    $this->setTargetBundle('article');

    parent::__construct($group);
  }

  /**
   * {@inheritdoc}
   */
  public function addCustomFieldMapping() {
    // Map the custom fields.
    $this->addFieldMapping('c4m_body', 'body')
         ->arguments(array('format' => 'full_html'));
    $this->addFieldMapping('c4m_has_video', 'has_video')
         ->defaultValue(0);
    $this->addFieldMapping('c4m_related_topic', 'topics')
         ->sourceMigration('C4dMigrateImportNodeTopic');
    $this->addFieldMapping('c4m_vocab_geo', 'geo')
         ->sourceMigration('C4dMigrateImportVocabGeo')
         ->arguments(array('source_type' => 'tid'));
    $this->addFileFieldMapping('c4m_media', 'teaser_image');
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
    $row->topics = unserialize($row->topics);
    $row->geo    = unserialize($row->geo);

    // Prepare the picture data.
    $this->prepareFileInRow($row, 'teaser_image');

    $row->body = $this->prepareHtml($row->body);

    return TRUE;
  }
}
