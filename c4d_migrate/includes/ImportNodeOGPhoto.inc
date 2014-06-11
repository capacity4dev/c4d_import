<?php
/**
 * @file
 * Migrate class to import the Photo group nodes.
 */

/**
 * Class to import the Photo group nodes from the Drupal 6 platform.
 */
class C4dMigrateImportNodeOGPhoto extends C4dMigrateImportBaseNodeOG {
  /**
   * Configure the object during creation.
   *
   * @param MigrateGroup $group
   *   The migrate Group object.
   */
  public function __construct($group = NULL) {
    $this->description = t('Import Photo group nodes.');
    $this->setSourceTable('c4d_export_node_node_gallery_image');
    $this->setTargetBundle('photo');

    $this->setCategoriesAndTags(FALSE);

    $this->dependencies[] = 'C4dMigrateImportNodeOGPhotoalbum';

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
    $this->addFileFieldMapping('c4m_media', 'field_node_gallery_image');

    $this->addFieldMapping('node_gallery_ref_1', 'gid')
      ->sourceMigration('C4dMigrateImportNodeOGPhotoAlbum');
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
    $this->prepareFileInRow($row, 'field_node_gallery_image');

    return TRUE;
  }

  /**
   * Review a data row after fetch, returning FALSE to skip it.
   *
   * @param object $row
   *   The data row we are importing.
   */
  public function complete($node, $row) {
    module_load_include('module', 'node_gallery_api');

    $items = field_get_items('node', $node, 'node_gallery_ref_1');

    $gallery_id = $items[0]['target_id'];

    $relationships = node_gallery_api_get_relationships($gallery_id, $node->nid);

    foreach ($relationships as $relationship) {
      if ($row->weight != $relationship->weight) {
        $relationship->weight = $row->weight;
        $relationship->save();
      }
    }

    // Set cover
    if ($row->is_cover == 1) {
      node_gallery_api_set_gallery_cover_item($node->nid, $gallery_id);
    }
  }
}
