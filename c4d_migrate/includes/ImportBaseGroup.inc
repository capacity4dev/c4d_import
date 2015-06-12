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
  public $dependencies
    = array(
      'C4dMigrateImportUsers',
      'C4dMigrateCreateCSVNodeOrganisations',
      'C4dMigrateCreateCSVTermTopic',
    );

  /**
   * Add the mapping for the common node fields.
   */
  public function addDefaultFieldMapping() {
    parent::addDefaultFieldMapping();

    $this->addFieldMapping('purl', 'purl');


    $this->addFieldMapping('c4m_vocab_topic', 'field_ref_topic')
      ->arguments(array('source_type' => 'tid'));

    $this->addFieldMapping('c4m_vocab_geo', 'taxonomy')
      ->arguments(array('source_type' => 'tid'));

    $this->addFieldMapping(OG_GROUP_FIELD, NULL)->defaultValue(TRUE);
    $this->addFieldMapping('og_description', 'og_description');

    // Map the picture field.
    $this->addFileFieldMapping('c4m_media', 'field_og_image');
    // @TODO Hide EC Field not mapped (field_hide_ec_disclaimer).

    // Map the access (public/private).
    $this->addFieldMapping('group_access', 'og_private');

    // Map the group status.
    $this->addFieldMapping('c4m_og_status', '_c4m_og_status');

    $this->addFieldMapping('field_membership_open_request', '_c4m_og_selective')
      ->defaultValue(0);
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

    $topic_nids = unserialize($row->field_ref_topic);
    $topics = array();

    foreach($topic_nids as $nid) {
      $title = $this->getNodeTitleFromNid($nid);

      $mapped = c4m_vocab_topic_get_mapping_by_source($title);

      foreach($mapped as $term) {
        $topics[] = $term->tid;
      }
    }

    $row->field_ref_topic = $topics;

    // Prepare the picture data.
    $this->prepareFileInRow($row, 'field_og_image');

    // Prepare the group status.
    $this->prepareC4mOgStatus($row);

    $row->_c4m_og_selective = $row->og_selective == 0 ? 1 : 0;

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
   * @param object $entity
   * @param object $row
   */
  public function complete($entity, $row) {
    parent::complete($entity, $row);

    $wiki = node_load($entity->nid);

    db_update('node')->fields(array('created' => $entity->created))
      ->condition('nid', $wiki->nid, '=')
      ->execute();
    db_update('node')->fields(array('changed' => $entity->created))
      ->condition('nid', $wiki->nid, '=')
      ->execute();
    db_update('node_revision')->fields(
      array('timestamp' => $entity->created, 'uid' => $entity->uid)
    )
      ->condition('nid', $wiki->nid, '=')
      ->condition('vid', $wiki->vid, '=')
      ->execute();
  }

  /**
   * Helper to create a pluggable node access record based on the og_spaces_type
   * value.
   *
   * @param stdClass $row
   *
   * @param stdClass $entity
   */
  public function savePluggableNodeAccess(&$group, $row) {
    $row->_pluggable_node_access = NULL;

    $organisation_eu = $this->handleSourceMigration(
      'C4dMigrateCreateCSVNodeOrganisations',
      'organisation_eu',
      NULL,
      $this
    );
    $organisation_ec = $this->handleSourceMigration(
      'C4dMigrateCreateCSVNodeOrganisations',
      'organisation_ec',
      NULL,
      $this
    );

    // Mapping between C4D & C4M group access types.
    $organisations_map = array(
      'ec_og' => array(
        $organisation_ec
      ),
      'eu_og' => array(
        $organisation_eu,
        $organisation_ec
      ),
    );
    if (!isset($organisations_map[$row->og_spaces_type])) {
      return;
    }

    // Create the pluggable node access record.
    $pluggable_node_access = c4m_og_save_pluggable_node_access_restricted(
      $group,
      $organisations_map[$row->og_spaces_type]
    );

    if ($pluggable_node_access) {
      $group->group_access = array(
        LANGUAGE_NONE => array(
          0 => array(
            'value' => 1
          )
        )
      );

      $group->pluggable_node_access = array(LANGUAGE_NONE => array());
      foreach ($pluggable_node_access as $na) {
        $group->pluggable_node_access[LANGUAGE_NONE][] = array(
          'target_id' => $na->id
        );
      }

      $group->revision = FALSE;

      $this->nodeSave($group);
    }
  }

  /**
   * Helper to create a status record based on the published value.
   *
   * @param stdClass $row
   */
  public function prepareC4mOgStatus($row) {
    // Mapping between C4D & C4M group statuses.
    $status_map = array(
      NODE_NOT_PUBLISHED => 'draft',
      NODE_PUBLISHED     => 'published',
    );
    $row->_c4m_og_status = $status_map[$row->status];
  }

  /**
   * {inheritDoc}
   */
  public function postImport() {
    parent::postImport();

    // Data is way too big to do node access rebuild via command line.
    // As it does not support batch API.
    // Therefore we set a flag that will indicate a rebuild is needed.
    node_access_needs_rebuild(TRUE);

    c4m_og_reset_restricted_info();
  }
}
