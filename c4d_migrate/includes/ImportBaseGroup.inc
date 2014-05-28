<?php
/**
 * @file
 * Base class to import group type data into nodes.
 */

/**
 * Base class to import group type data into nodes.
 */
abstract class C4dMigrateImportBaseGroup extends C4dMigrateImportBaseNode {
  /**
   * Creation classes that should run before this one.
   *
   * @var array
   */
  public $dependencies = array(
    'C4dMigrateImportUsers',
    'C4dMigrateImportNodeArticle',
  );

  /**
   * Add the mapping for the common node fields.
   */
  public function addDefaultFieldMapping() {
    parent::addDefaultFieldMapping();

    $this->addFieldMapping('c4m_related_topic', 'field_ref_topic')
         ->sourceMigration('C4dMigrateImportNodeTopic');

    $this->addFieldMapping('c4m_vocab_geo', 'taxonomy')
         ->sourceMigration('C4dMigrateImportVocabGeo')
         ->arguments(array('source_type' => 'tid'));

    // Map the picture field.
    $this->addFileFieldMapping('c4m_media', 'field_og_image');

    // Hide EC Field not mapped.
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


    $row->taxonomy = unserialize($row->taxonomy);
    $row->field_ref_topic = unserialize($row->field_ref_topic);

    // Prepare the picture data.
    $this->prepareFileInRow($row, 'field_og_image');

    return TRUE;
  }
}
