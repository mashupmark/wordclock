<script lang="ts" setup>
import * as v from "valibot";
import SettingsSection from "./SettingsSection.vue";

const toast = useToast();

const schema = v.object({
  SSID: v.pipe(v.string(), v.trim(), v.nonEmpty("required")),
  PW: v.pipe(v.string(), v.trim(), v.nonEmpty("required")),
});
type Schema = v.InferOutput<typeof schema>;

const state = reactive<Schema>({ SSID: "", PW: "" });

const isSaving = ref(false);
const onSubmit = async () => {
  try {
    isSaving.value = true;
    await $fetch("/api/settings/wifi", { method: "POST", body: state });
  } catch {
    toast.add({
      title: "Error",
      description: "Failed to update wifi credentials",
      color: "error",
    });
  } finally {
    isSaving.value = false;
  }
};
</script>

<template>
  <SettingsSection
    title="Wifi"
    description="Update the wifi credentials used to connect to the internet"
    :schema
    :state
    :isSaving
    @submit="onSubmit"
  >
    <UFormField label="SSID" required>
      <UInput v-model="state.SSID" class="w-full" />
    </UFormField>
    <UFormField label="Password" required>
      <UInput
        class="w-full"
        v-model="state.PW"
        type="password"
        trailing-icon="i-heroicons-lock-closed"
      />
    </UFormField>
  </SettingsSection>
</template>
