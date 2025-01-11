<script lang="ts" setup>
import { useMessage, NButton, NForm, NFormItem, NInput } from "naive-ui";

const message = useMessage();
const formValue = ref({ SSID: "", PW: "" });

const { status, execute: submit } = useAsyncData(
  async () => {
    try {
      await $fetch("/api/wifi", { method: "PUT", body: formValue.value });
      message.success("Successfully updated Wifi credentials");
    } catch {
      message.error("Failed to update Wifi credentials");
    }
  },
  { immediate: false }
);
</script>

<template>
  <NForm
    class="form"
    :model="formValue"
    :show-feedback="false"
    :disabled="status === 'pending'"
  >
    <NFormItem label="SSID" required>
      <NInput v-model:value="formValue.SSID" placeholder="SSID" />
    </NFormItem>
    <NFormItem label="Password">
      <NInput
        v-model:value="formValue.PW"
        placeholder="Password"
        type="password"
        show-password-on="click"
      />
    </NFormItem>
    <NFormItem class="form__submit" :show-label="false">
      <NButton
        :loading="status === 'pending'"
        type="primary"
        attr-type="submit"
        @click="submit()"
      >
        Save
      </NButton>
    </NFormItem>
  </NForm>
</template>

<style lang="scss" scoped>
.form {
  display: flex;
  flex-direction: column;
  gap: 0.5rem;
}
</style>
