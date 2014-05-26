<?php
/**
 * @file
 * Migrate class to import the Region nodes.
 */

/**
 * Class to import the Region nodes from the Drupal 6 platform.
 */
class C4dMigrateImportNodeRegion extends C4dMigrateImportBaseNode {
  /**
   * Creation classes that should run before this one.
   *
   * @var array
   */
  public $dependencies = array(
    'C4dMigrateImportUsers',
    'C4dMigrateImportVocabGeo',
  );


  /**
   * Configure the object during creation.
   *
   * @param MigrateGroup $group
   *   The migrate Group object.
   */
  public function __construct($group = NULL) {
    $this->description = t('Import Region nodes.');
    $this->setSourceTable('c4d_export_node_geo_region');
    $this->setTargetBundle('region');

    parent::__construct($group);
  }

  /**
   * {@inheritdoc}
   */
  public function addCustomFieldMapping() {
    // Map the custom fields.
    $this->addFieldMapping('c4m_body', 'body')
      ->arguments(array('format' => 'full_html'));
    $this->addFieldMapping('c4m_vocab_geo', 'taxonomy')
      ->sourceMigration('C4dMigrateImportVocabGeo')
      ->arguments(array('source_type' => 'tid'));

    // Map the picture field.
    $this->addFileFieldMapping('c4m_media', 'field_picture_region');
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
    $row->field_links = unserialize($row->field_links);
    $row->field_documents = unserialize($row->field_documents);
    $row->field_group_suggestions = unserialize($row->field_group_suggestions);
    $row->taxonomy = unserialize($row->taxonomy);

    // Prepare the picture data.
    $this->prepareFileInRow($row, 'field_picture_region', 'files');

    return TRUE;
  }
}
