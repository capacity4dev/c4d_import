<?php
/**
 * @file
 * Migrate class to import the Wiki pages.
 */

/**
 * Class to import the Wiki pages from the Drupal 6 platform.
 */
class C4dMigrateImportNodeOGMinisite extends C4dMigrateImportBaseNodeOG {
  /**
   * Configure the object during creation.
   *
   * @param MigrateGroup $group
   *   The migrate Group object.
   */
  public function __construct($group = NULL) {
    $this->description = t('Import Wiki pages.');
    $this->setSourceTable('c4d_export_node_minisite_page');
    $this->setTargetBundle('wiki_page');

    $this->setCategoriesAndTags(TRUE);

    parent::__construct($group);

    // Override query to sort based on parent.
    $query        = $this->createSelect($this->getSourceTable(), 'node')
                         ->fields('node')
                         ->orderBy('parent, nid');
    $this->source = new MigrateSourceSQL(
      $query,
      array(),
      NULL,
      array('map_joinable' => FALSE)
    );
  }

  /**
   * {@inheritdoc}
   */
  public function addCustomFieldMapping() {
    // Map the custom fields.
    $this->addFieldMapping('c4m_body', 'body')
         ->arguments(array('format' => 'full_html'));
  }

  /**
   * Review a data row after fetch, returning FALSE to skip it.
   *
   * @param object $node
   *   The node object we are creating.
   * @param object $row
   *   The data row we are importing.
   */
  public function complete($node, $row) {
    // Books are not loaded by default.
    $node = node_load($node->nid);

    // Take care of import --update.
    // Now there's an Integrity constraint violation
    // Duplicate entry for key nid.
    $update = FALSE;
    if (!empty($node->book) && !empty($node->book['nid'])) {
      if ($node->book['nid'] !== 'new' || intval($node->book['nid']) != 0) {
        $update = TRUE;
      }
    }
    $og_id = $row->{OG_AUDIENCE_FIELD}[0];

    // If no parent - we need to retrieve the master page.
    $parent = $row->parent;
    if (empty($parent)) {
      // No parent: this is a direct child of the master page.
      if (!$update) {
        // Retrieve bid and parent mlid of the minisite master page.
        module_load_include('module', 'c4m_content_wiki');
        $parent = node_load($og_id);

        $book           = $parent->book;
        $book['weight'] = $row->weight;
        $book['nid']    = $node->nid;
        $book['plid']   = $book['mlid'];
        unset($book['mlid']);

        $node->book = $book;

        $this->nodeSave($node, $row->changed);

      }
      else {
        // This is an update, so we retrieve the existing book.
        $book = $node->book;
        // Only value for the master book page that can change is the weight.
        if ($book['weight'] != $row->weight) {
          $book['weight'] = $row->weight;
          $node->book     = $book;
          $this->nodeSave($node, $row->changed);

        }

      }
    }
    else {
      // Create a nested page.
      $parent_nid = $this->getParentNid($parent);
      $parent     = node_load($parent_nid);

      $book = $parent->book;

      $book['plid'] = $book['mlid'];
      $book['nid']  = $node->nid;

      if ($update) {
        // There is already a {menu_links} entry for this node.
        $book['mlid'] = $this->getMlidEntry($node->nid);
      }
      else {
        unset($book['mlid']);
      }
      $book['weight'] = $row->weight;

      $node->book = $book;

      _book_update_outline($node);
      $this->nodeSave($node, $row->changed);

    }

    parent::complete($node, $row);
  }

  /**
   * Get the migrated parent id.
   *
   * @param int $nid
   *   Source id of the parent.
   *
   * @return int|null
   *   Parent id or NULL.
   */
  private function getParentNid($nid) {
    $parent = $this->handleSourceMigration(
                   'C4dMigrateImportNodeOGMinisite',
                     $nid,
                     NULL,
                     $this
    );
    if (!empty($parent)) {
      return $parent;
    }

    return NULL;
  }

  /**
   * Based on the node id, retrieve the existing {menu_links} mlid.
   *
   * @param int $nid
   *   Node id to retrieve the mlid for.
   *
   * @return int|NULL
   *   ID of the {menu_links} entry or NULL.
   */
  private function getMlidEntry($nid) {
    $q   = db_select('book', 'book')
           ->fields('book', array('mlid'))
           ->condition('book.nid', $nid, '=')
           ->range(0, 1);
    $res = $q->execute()->fetchObject();

    if (!empty($res->mlid)) {
      return $res->mlid;
    }

    return NULL;
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

    $row->body = $this->prepareHtml($row->body);
  }
}
