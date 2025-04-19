<script lang="ts" setup>
import * as v from "valibot";
import SettingsSection from "./SettingsSection.vue";
import { timeZonesNames } from "@vvo/tzdb";
import type { GeneralSettings } from "~~/shared/types/api";

const toast = useToast();

const schema = v.object({ timezone: v.string() });
type Schema = v.InferOutput<typeof schema>;

const state = reactive<Schema>({ timezone: "" });

const { status, data } = useFetch<GeneralSettings>("/api/settings", {
  server: false,
});
watchEffect(() => {
  if (!data.value) return;
  state.timezone = data.value.timezone;
});

const { status: saveStatus, execute: save } = useAsyncData(
  async () => {
    try {
      await $fetch("/api/settings", {
        method: "POST",
        body: { timezone: state.timezone } satisfies GeneralSettings,
      });
    } catch {
      toast.add({ title: "Failed to update time zone", color: "error" });
    }
  },
  { immediate: false }
);
</script>

<template>
  <SettingsSection
    title="Time zone"
    description="Set the time zone the time should be displayed in (requires a restart to apply)"
    :schema
    :state
    :isSaving="saveStatus === 'pending'"
    @submit="save()"
  >
    <UFormField label="Time zone" required>
      <USelectMenu
        class="w-full"
        v-model="state.timezone"
        :items="timeZonesNames"
        :loading="status === 'pending'"
        required
      />
    </UFormField>
  </SettingsSection>
</template>
