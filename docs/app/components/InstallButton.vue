<script setup lang="ts">
defineProps<{ manifest: string }>();
</script>

<template>
  <ClientOnly>
    <esp-web-install-button :manifest>
      <UButton
        slot="activate"
        class="font-bold rounded-full px-6 py-3 cursor-pointer"
        label="Connect"
        size="xl"
      />
    </esp-web-install-button>
    <template #fallback>
      <UButton
        slot="activate"
        class="font-bold rounded-full px-6 py-3 cursor-pointer"
        label="Connect"
        size="xl"
      />
    </template>
  </ClientOnly>
</template>
