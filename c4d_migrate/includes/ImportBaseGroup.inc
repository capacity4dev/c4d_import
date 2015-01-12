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
      ->arguments(array('source_type' => 'tid'));

    $this->addFieldMapping(OG_GROUP_FIELD, NULL)->defaultValue(TRUE);
    $this->addFieldMapping('og_description', 'og_description');

    // Map the picture field.
    $this->addFileFieldMapping('c4m_media', 'field_og_image');
    // @TODO Hide EC Field not mapped (field_hide_ec_disclaimer).

    // Map the access (public/private).
    $this->addFieldMapping('group_access', 'og_private');
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
    $row->taxonomy = $this->mapTaxonomyVocabWithParent(
      $this->mapTaxonomyVocabRegionCountries($row->taxonomy, 'c4m_vocab_geo')
    );

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
    $wiki = node_load($entity->nid);

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

  /**
   * Helper to create a pluggable node access record based on the og_spaces_type
   * value.
   *
   * @param stdClass $row
   */
  public function preparePluggableNodeAccess($row) {
    $row->_pluggable_node_access = null;

    // Mapping between C4D & C4M group access types.
    $organisations_map = array(
      'ec_og' => 'ec',
      'eu_og' => 'eu',
    );
    if (!isset($organisations_map[$row->og_spaces_type])) {
      return;
    }

    // Dummy node.
    $values = array(
      'type' => $this->getTargetBundle(),
      'uid' => 1,
      'status' => 1,
      'comment' => 1,
      'promote' => 0,
    );
    $group = entity_create('node', $values);

    // Create the pluggable node access record.
    $pluggable_node_access = c4m_og_save_pluggable_node_access_restricted(
      $group,
      array($organisations_map[$row->og_spaces_type])
    );

    if ($pluggable_node_access) {
      $row->_pluggable_node_access = $pluggable_node_access->id;
    }
  }
}
