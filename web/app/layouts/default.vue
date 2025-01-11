<script lang="ts" setup>
import { Wifi } from "@vicons/ionicons5";
import {
  NButton,
  NIcon,
  NLayout,
  NLayoutContent,
  NLayoutSider,
} from "naive-ui";

const isCollapsed = ref(true);
</script>

<template>
  <NLayout position="absolute" has-sider>
    <NLayoutSider
      v-model:collapsed="isCollapsed"
      :content-class="`layout__sidebar${isCollapsed ? '--collapsed' : ''}`"
      collapse-mode="width"
      :width="128"
      :collapsed-width="64"
      show-trigger="arrow-circle"
      bordered
    >
      <NButton text class="sidebar__button">
        <template #icon>
          <NIcon :size="32"><Wifi /></NIcon>
        </template>
        <span v-if="!isCollapsed">WiFi</span>
      </NButton>
    </NLayoutSider>
    <NLayoutContent content-class="layout__content">
      <slot />
    </NLayoutContent>
  </NLayout>
</template>

<style lang="scss" scoped>
@mixin sidebar-layout {
  display: flex;
  flex-direction: column;
  align-items: start;
  padding: 1.5rem;
}

:deep(.layout__sidebar) {
  @include sidebar-layout;
}

:deep(.layout__sidebar--collapsed) {
  @include sidebar-layout;
}

.sidebar__button {
  justify-content: start;
  gap: 0.5rem;
}

:deep(.layout__content) {
  padding: 1rem;
}
</style>
