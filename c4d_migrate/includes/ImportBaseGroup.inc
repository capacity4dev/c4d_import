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
    'C4dMigrateImportNodeTopic',
  );

  /**
   * Add the mapping for the common node fields.
   */
  public function addDefaultFieldMapping() {
    parent::addDefaultFieldMapping();

    $this->addFieldMapping('purl', 'purl');

    $this->addFieldMapping('c4m_related_topic', 'field_ref_topic')
      ->sourceMigration('C4dMigrateImportNodeTopic');

    $this->addFieldMapping('c4m_vocab_geo', 'taxonomy')
      ->sourceMigration('C4dMigrateImportVocabGeo')
      ->arguments(array('source_type' => 'tid'));

    $this->addFieldMapping(OG_GROUP_FIELD, NULL)->defaultValue(TRUE);
    $this->addFieldMapping('og_description', 'og_description');

    // Map the picture field.
    $this->addFileFieldMapping('c4m_media', 'field_og_image');
    // @TODO Hide EC Field not mapped (field_hide_ec_disclaimer).
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

  /**
   * Review a data row after fetch, returning FALSE to skip it.
   *
   * @param object $row
   *   The data row we are importing.
   */
  public function prepare($node, $row) {
    $node->purl = array('value' => $row->purl);
  }

  /**
   * Review a data row after fetch, returning FALSE to skip it.
   *
   * @param object $row
   *   The data row we are importing.
   */
  public function complete($entity, $row) {
    $wiki = c4m_content_wiki_get_book_by_gid($entity->nid);

    db_update('node')->fields(array('created' => $entity->created))
      ->condition('nid', $wiki->nid, '=')
      ->execute();
    db_update('node')->fields(array('changed' => $entity->created))
      ->condition('nid', $wiki->nid, '=')
      ->execute();
    db_update('node_revision')->fields(array('timestamp' => $entity->created, 'uid' => $entity->uid))
      ->condition('nid', $wiki->nid, '=')
      ->condition('vid', $wiki->vid, '=')
      ->execute();
  }
}
