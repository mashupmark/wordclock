<script lang="ts" setup>
import * as v from "valibot";
import SettingsSection from "./SettingsSection.vue";
import type { GeneralSettings } from "~~/shared/types/api";

const toast = useToast();

const schema = v.object({
  brightness: v.pipe(v.number(), v.integer(), v.minValue(0), v.maxValue(255)),
});
type Schema = v.InferOutput<typeof schema>;

const state = reactive<Schema>({ brightness: 255 });

const { status, data } = useFetch<GeneralSettings>("/api/settings", {
  server: false,
});
watchEffect(() => {
  if (!data.value) return;
  state.brightness = data.value.brightness;
});

const { status: saveStatus, execute: save } = useAsyncData(
  async () => {
    try {
      await $fetch("/api/settings", {
        method: "POST",
        body: {
          brightness: state.brightness,
        } satisfies Partial<GeneralSettings>,
      });
    } catch {
      toast.add({ title: "Failed to update brightness", color: "error" });
    }
  },
  { immediate: false }
);
</script>

<template>
  <SettingsSection
    title="Brightness"
    description="Set the brightness of the leds"
    :schema
    :state
    :isSaving="saveStatus === 'pending'"
    @submit="save()"
  >
    <UFormField label="Brightness" required>
      <USlider
        v-model="state.brightness"
        :min="0"
        :max="255"
        :disabled="status === 'pending'"
      />
    </UFormField>
  </SettingsSection>
</template>
