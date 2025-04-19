<script lang="ts" setup>
import * as v from "valibot";
import SettingsSection from "./SettingsSection.vue";
import type { ClockSettings } from "~~/shared/types/api";

const toast = useToast();

const schema = v.object({
  color: v.pipe(v.string(), v.hexColor()),
});
type Schema = v.InferOutput<typeof schema>;

const state = reactive<Schema>({ color: "#ff0000" });

const { status, data } = useFetch<ClockSettings>("/api/clock", {
  server: false,
});
watchEffect(() => {
  if (!data.value) return;
  state.color = data.value.animation.Static.color;
});

const { status: saveStatus, execute: save } = useAsyncData(
  async () => {
    try {
      await $fetch("/api/clock", {
        method: "POST",
        body: {
          animation: { Static: { name: "static", color: state.color } },
        } satisfies ClockSettings,
      });
    } catch {
      toast.add({ title: "Failed to update color", color: "error" });
    }
  },
  { immediate: false }
);
</script>

<template>
  <SettingsSection
    title="Clock Color"
    description="Set the color the time is displayed in"
    :schema
    :state
    :isSaving="saveStatus === 'pending'"
    @submit="save()"
  >
    <UFormField label="Color" required>
      <UInput
        class="w-full"
        :ui="{ base: 'h-8 p-0' }"
        v-model="state.color"
        type="color"
        variant="none"
        :loading="status === 'pending'"
      />
    </UFormField>
  </SettingsSection>
</template>
