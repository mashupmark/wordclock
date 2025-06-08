<script setup lang="ts">
const props = defineProps<{ manifest: string }>();

const runtimeConfig = useRuntimeConfig();
const toast = useToast();

const manifest = computed(
  () => `${runtimeConfig.app.baseURL}${props.manifest}`
);

const unsupportedDescription =
  "Your browser is not supported, please use e.g. Google Chrome or Microsoft Edge";
const showUnsupportedToast = () => {
  toast.add({
    color: "error",
    title: "Browser not supported",
    description: unsupportedDescription,
  });
};
</script>

<template>
  <ClientOnly>
    <esp-web-install-button :manifest>
      <UButton
        slot="activate"
        class="font-bold rounded-full px-6 py-3 cursor-pointer"
        label="Connect"
      />
      <UButton
        slot="unsupported"
        class="font-bold rounded-full px-6 py-3 cursor-not-allowed"
        label="Connect"
        color="error"
        variant="soft"
        :title="unsupportedDescription"
        @click="showUnsupportedToast"
      />
    </esp-web-install-button>
    <template #fallback>
      <UButton
        slot="activate"
        class="font-bold rounded-full px-6 py-3 cursor-pointer"
        label="Connect"
      />
    </template>
  </ClientOnly>
</template>
